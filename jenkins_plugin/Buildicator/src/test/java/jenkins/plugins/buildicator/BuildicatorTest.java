package jenkins.plugins.buildicator;

import com.cloudbees.plugins.credentials.common.StandardUsernamePasswordCredentials;
import hudson.EnvVars;
import hudson.model.FreeStyleBuild;
import hudson.model.FreeStyleProject;
import hudson.slaves.EnvironmentVariablesNodeProperty;
import hudson.tasks.BuildStepMonitor;
import hudson.tasks.Shell;
import jenkins.plugins.buildicator.Buildicator;

import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.jvnet.hudson.test.JenkinsRule;

import java.io.IOException;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

@Ignore("TODO Figure out the best way to test the plugin.")
public class BuildicatorTest {

    @Rule
    public JenkinsRule j = new JenkinsRule();

    private Buildicator createSubject() {
        return new Buildicator(
            "brokerUrl",
            "topic1",
            "DEVICE001",
            "1",
            false,
            null
        );
    }

    @Test
    public void canInitiateNewNotifier() {
        Buildicator notifier = createSubject();
        assertNotNull(notifier);
    }

    @Test
    public void canGetAllFields() {
        Buildicator notifier = createSubject();
        assertEquals("topic1", notifier.getTopic());
        assertEquals("DEVICE001", notifier.getMessage());
        assertEquals("brokerUrl", notifier.getBrokerUrl());
        assertEquals(1, notifier.getQos());
        assertEquals(BuildStepMonitor.NONE, notifier.getRequiredMonitorService());
    }

    @Test
    public void testNotification() throws Exception{
        FreeStyleProject project = j.createFreeStyleProject();
        project.getBuildersList().add(new Shell("echo hello"));
        FreeStyleBuild build = project.scheduleBuild2(0).get();
        project.getPublishersList().add(createSubject());
        System.out.println(build.getDisplayName() + " completed");
    }

    public void setEnvironmentVariables() throws IOException {
        EnvironmentVariablesNodeProperty prop = new EnvironmentVariablesNodeProperty();
        EnvVars envVars = prop.getEnvVars();
        envVars.put("sampleEnvVarKey", "sampleEnvVarValue");
        j.jenkins.getGlobalNodeProperties().add(prop);
    }

}
